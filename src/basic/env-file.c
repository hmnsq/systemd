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

प्रकारव्याख्या पूर्णांक (*ढकला_env_func_t)(
                स्थिरांक अक्षर *filename,
                अचिन्हित ओळ,
                स्थिरांक अक्षर *किल्ली,
                अक्षर *मूल्य,
                शून्य *userdata);

स्थिर पूर्णांक parse_env_file_internal(
                खाता *f,
                स्थिरांक अक्षर *fname,
                ढकला_env_func_t ढकला,
                शून्य *userdata) {

        माप_प्रकार n_किल्ली = 0, n_मूल्य = 0, last_मूल्य_whitespace = SIZE_MAX, last_किल्ली_whitespace = SIZE_MAX;
        _cleanup_free_ अक्षर *contents = रिक्त, *किल्ली = रिक्त, *मूल्य = रिक्त;
        अचिन्हित ओळ = 1;
        पूर्णांक r;

        गणना {
                PRE_किल्ली,
                किल्ली_2,
                PRE_मूल्य,
                मूल्य_2,
                मूल्य_ESCAPE,
                SINGLE_QUOTE_मूल्य,
                DOUBLE_QUOTE_मूल्य,
                DOUBLE_QUOTE_मूल्य_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_किल्ली;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ढकला);

        जर (f)
                r = read_full_stream(f, &contents, रिक्त);
        अन्यथा
                r = read_full_file(fname, &contents, रिक्त);
        जर (r < 0)
                परत r;

        साठी (अक्षर *p = contents; *p; p++) {
                अक्षर c = *p;

                निवड (state) {

                स्थिती PRE_किल्ली:
                        जर (म_अक्षर(COMMENTS, c))
                                state = COMMENT;
                        अन्यथा जर (!म_अक्षर(WHITESPACE, c)) {
                                state = किल्ली_2;
                                last_किल्ली_whitespace = SIZE_MAX;

                                जर (!GREEDY_REALLOC(किल्ली, n_किल्ली+2))
                                        परत -ENOMEM;

                                किल्ली[n_किल्ली++] = c;
                        }
                        थांबा;

                स्थिती किल्ली_2:
                        जर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_किल्ली;
                                ओळ++;
                                n_किल्ली = 0;
                        } अन्यथा जर (c == '=') {
                                state = PRE_मूल्य;
                                last_मूल्य_whitespace = SIZE_MAX;
                        } अन्यथा {
                                जर (!म_अक्षर(WHITESPACE, c))
                                        last_किल्ली_whitespace = SIZE_MAX;
                                अन्यथा जर (last_किल्ली_whitespace == SIZE_MAX)
                                         last_किल्ली_whitespace = n_किल्ली;

                                जर (!GREEDY_REALLOC(किल्ली, n_किल्ली+2))
                                        परत -ENOMEM;

                                किल्ली[n_किल्ली++] = c;
                        }

                        थांबा;

                स्थिती PRE_मूल्य:
                        जर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_किल्ली;
                                ओळ++;
                                किल्ली[n_किल्ली] = 0;

                                जर (मूल्य)
                                        मूल्य[n_मूल्य] = 0;

                                /* strip trailing whitespace from key */
                                जर (last_किल्ली_whitespace != SIZE_MAX)
                                        किल्ली[last_किल्ली_whitespace] = 0;

                                r = ढकला(fname, ओळ, किल्ली, मूल्य, userdata);
                                जर (r < 0)
                                        परत r;

                                n_किल्ली = 0;
                                मूल्य = रिक्त;
                                n_मूल्य = 0;

                        } अन्यथा जर (c == '\'')
                                state = SINGLE_QUOTE_मूल्य;
                        अन्यथा जर (c == '"')
                                state = DOUBLE_QUOTE_मूल्य;
                        अन्यथा जर (c == '\\')
                                state = मूल्य_ESCAPE;
                        अन्यथा जर (!म_अक्षर(WHITESPACE, c)) {
                                state = मूल्य_2;

                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        परत -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        थांबा;

                स्थिती मूल्य_2:
                        जर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_किल्ली;
                                ओळ++;

                                किल्ली[n_किल्ली] = 0;

                                जर (मूल्य)
                                        मूल्य[n_मूल्य] = 0;

                                /* Chomp off trailing whitespace from value */
                                जर (last_मूल्य_whitespace != SIZE_MAX)
                                        मूल्य[last_मूल्य_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                जर (last_किल्ली_whitespace != SIZE_MAX)
                                        किल्ली[last_किल्ली_whitespace] = 0;

                                r = ढकला(fname, ओळ, किल्ली, मूल्य, userdata);
                                जर (r < 0)
                                        परत r;

                                n_किल्ली = 0;
                                मूल्य = रिक्त;
                                n_मूल्य = 0;

                        } अन्यथा जर (c == '\\') {
                                state = मूल्य_ESCAPE;
                                last_मूल्य_whitespace = SIZE_MAX;
                        } अन्यथा {
                                जर (!म_अक्षर(WHITESPACE, c))
                                        last_मूल्य_whitespace = SIZE_MAX;
                                अन्यथा जर (last_मूल्य_whitespace == SIZE_MAX)
                                        last_मूल्य_whitespace = n_मूल्य;

                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        परत -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        थांबा;

                स्थिती मूल्य_ESCAPE:
                        state = मूल्य_2;

                        जर (!म_अक्षर(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        परत -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }
                        थांबा;

                स्थिती SINGLE_QUOTE_मूल्य:
                        जर (c == '\'')
                                state = PRE_मूल्य;
                        अन्यथा {
                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        परत -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        थांबा;

                स्थिती DOUBLE_QUOTE_मूल्य:
                        जर (c == '"')
                                state = PRE_मूल्य;
                        अन्यथा जर (c == '\\')
                                state = DOUBLE_QUOTE_मूल्य_ESCAPE;
                        अन्यथा {
                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        परत -ENOMEM;

                                मूल्य[n_मूल्य++] = c;
                        }

                        थांबा;

                स्थिती DOUBLE_QUOTE_मूल्य_ESCAPE:
                        state = DOUBLE_QUOTE_मूल्य;

                        जर (म_अक्षर(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+2))
                                        परत -ENOMEM;
                                मूल्य[n_मूल्य++] = c;
                        } अन्यथा जर (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                जर (!GREEDY_REALLOC(मूल्य, n_मूल्य+3))
                                        परत -ENOMEM;
                                मूल्य[n_मूल्य++] = '\\';
                                मूल्य[n_मूल्य++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        थांबा;

                स्थिती COMMENT:
                        जर (c == '\\')
                                state = COMMENT_ESCAPE;
                        अन्यथा जर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_किल्ली;
                                ओळ++;
                        }
                        थांबा;

                स्थिती COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        जर (म_अक्षर(NEWLINE, c)) {
                                state = PRE_किल्ली;
                                ओळ++;
                        } अन्यथा
                                state = COMMENT;
                        थांबा;
                }
        }

        जर (IN_SET(state,
                   PRE_मूल्य,
                   मूल्य_2,
                   मूल्य_ESCAPE,
                   SINGLE_QUOTE_मूल्य,
                   DOUBLE_QUOTE_मूल्य,
                   DOUBLE_QUOTE_मूल्य_ESCAPE)) {

                किल्ली[n_किल्ली] = 0;

                जर (मूल्य)
                        मूल्य[n_मूल्य] = 0;

                जर (state == मूल्य_2)
                        जर (last_मूल्य_whitespace != SIZE_MAX)
                                मूल्य[last_मूल्य_whitespace] = 0;

                /* strip trailing whitespace from key */
                जर (last_किल्ली_whitespace != SIZE_MAX)
                        किल्ली[last_किल्ली_whitespace] = 0;

                r = ढकला(fname, ओळ, किल्ली, मूल्य, userdata);
                जर (r < 0)
                        परत r;

                मूल्य = रिक्त;
        }

        परत 0;
}

स्थिर पूर्णांक check_utf8ness_and_warn(
                स्थिरांक अक्षर *filename, अचिन्हित ओळ,
                स्थिरांक अक्षर *किल्ली, अक्षर *मूल्य) {

        प्रतिज्ञा(किल्ली);

        जर (!utf8_is_valid(किल्ली)) {
                _cleanup_free_ अक्षर *p = रिक्त;

                p = utf8_escape_invalid(किल्ली);
                परत log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), ओळ, p);
        }

        जर (मूल्य && !utf8_is_valid(मूल्य)) {
                _cleanup_free_ अक्षर *p = रिक्त;

                p = utf8_escape_invalid(मूल्य);
                परत log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), ओळ, किल्ली, p);
        }

        परत 0;
}

स्थिर पूर्णांक parse_env_file_ढकला(
                स्थिरांक अक्षर *filename, अचिन्हित ओळ,
                स्थिरांक अक्षर *किल्ली, अक्षर *मूल्य,
                शून्य *userdata) {

        स्थिरांक अक्षर *k;
        बहु_सूची aq, *ap = userdata;
        पूर्णांक r;

        प्रतिज्ञा(किल्ली);

        r = check_utf8ness_and_warn(filename, ओळ, किल्ली, मूल्य);
        जर (r < 0)
                परत r;

        va_copy(aq, *ap);

        पर्यंत ((k = बहु_तर्क(aq, स्थिरांक अक्षर *))) {
                अक्षर **v;

                v = बहु_तर्क(aq, अक्षर **);

                जर (streq(किल्ली, k)) {
                        बहु_पूर्ण(aq);
                        free_and_replace(*v, मूल्य);

                        परत 1;
                }
        }

        बहु_पूर्ण(aq);
        मुक्त(मूल्य);

        परत 0;
}

पूर्णांक parse_env_filev(
                खाता *f,
                स्थिरांक अक्षर *fname,
                बहु_सूची ap) {

        पूर्णांक r;
        बहु_सूची aq;

        प्रतिज्ञा(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ढकला, &aq);
        बहु_पूर्ण(aq);
        परत r;
}

पूर्णांक parse_env_file_fdv(पूर्णांक fd, स्थिरांक अक्षर *fname, बहु_सूची ap) {
        _cleanup_fclose_ खाता *f = रिक्त;
        बहु_सूची aq;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        जर (r < 0)
                परत r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ढकला, &aq);
        बहु_पूर्ण(aq);
        परत r;
}

पूर्णांक parse_env_file_sentinel(
                खाता *f,
                स्थिरांक अक्षर *fname,
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        प्रतिज्ञा(f || fname);

        बहु_शुरू(ap, fname);
        r = parse_env_filev(f, fname, ap);
        बहु_पूर्ण(ap);

        परत r;
}

पूर्णांक parse_env_file_fd_sentinel(
                पूर्णांक fd,
                स्थिरांक अक्षर *fname, /* only used for logging */
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        बहु_शुरू(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        बहु_पूर्ण(ap);

        परत r;
}

पूर्णांक parse_env_datav(
                स्थिरांक अक्षर *data,
                माप_प्रकार size,
                स्थिरांक अक्षर *fname, /* only used for logging */
                बहु_सूची ap) {

        प्रतिज्ञा(data);

        जर (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ खाता *f = fmemopen_unlocked((शून्य*) data, size, "r");
        जर (!f)
                परत -ENOMEM;

        परत parse_env_filev(f, fname, ap);
}

पूर्णांक parse_env_data_sentinel(
                स्थिरांक अक्षर *data,
                माप_प्रकार size,
                स्थिरांक अक्षर *fname, /* only used for logging */
                ...) {

        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        बहु_पूर्ण(ap);

        परत r;
}

स्थिर पूर्णांक load_env_file_ढकला(
                स्थिरांक अक्षर *filename, अचिन्हित ओळ,
                स्थिरांक अक्षर *किल्ली, अक्षर *मूल्य,
                शून्य *userdata) {

        अक्षर ***m = userdata;
        अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(किल्ली);

        r = check_utf8ness_and_warn(filename, ओळ, किल्ली, मूल्य);
        जर (r < 0)
                परत r;

        p = strjoin(किल्ली, "=", मूल्य);
        जर (!p)
                परत -ENOMEM;

        r = strv_env_replace_consume(m, p);
        जर (r < 0)
                परत r;

        मुक्त(मूल्य);
        परत 0;
}

पूर्णांक load_env_file(खाता *f, स्थिरांक अक्षर *fname, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **m = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ढकला, &m);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(m);
        परत 0;
}

स्थिर पूर्णांक load_env_file_ढकला_pairs(
                स्थिरांक अक्षर *filename, अचिन्हित ओळ,
                स्थिरांक अक्षर *किल्ली, अक्षर *मूल्य,
                शून्य *userdata) {

        अक्षर ***m = ASSERT_PTR(userdata);
        पूर्णांक r;

        प्रतिज्ञा(किल्ली);

        r = check_utf8ness_and_warn(filename, ओळ, किल्ली, मूल्य);
        जर (r < 0)
                परत r;

        /* Check if the key is present */
        साठी (अक्षर **t = *m; t && *t; t += 2)
                जर (streq(t[0], किल्ली)) {
                        जर (मूल्य)
                                परत free_and_replace(t[1], मूल्य);
                        अन्यथा
                                परत free_and_strdup(t+1, "");
                }

        r = strv_extend(m, किल्ली);
        जर (r < 0)
                परत r;

        जर (मूल्य)
                परत strv_ढकला(m, मूल्य);
        अन्यथा
                परत strv_extend(m, "");
}

पूर्णांक load_env_file_pairs(खाता *f, स्थिरांक अक्षर *fname, अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **m = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(f || fname);
        प्रतिज्ञा(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ढकला_pairs, &m);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(m);
        परत 0;
}

पूर्णांक load_env_file_pairs_fd(पूर्णांक fd, स्थिरांक अक्षर *fname, अक्षर ***ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        जर (r < 0)
                परत r;

        परत load_env_file_pairs(f, fname, ret);
}

स्थिर पूर्णांक merge_env_file_ढकला(
                स्थिरांक अक्षर *filename, अचिन्हित ओळ,
                स्थिरांक अक्षर *किल्ली, अक्षर *मूल्य,
                शून्य *userdata) {

        अक्षर ***env = ASSERT_PTR(userdata);
        अक्षर *expanded_मूल्य;
        पूर्णांक r;

        प्रतिज्ञा(किल्ली);

        जर (!मूल्य) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), ओळ, किल्ली);
                परत 0;
        }

        जर (!env_name_is_valid(किल्ली)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), ओळ, किल्ली);
                मुक्त(मूल्य);
                परत 0;
        }

        r = replace_env(मूल्य,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_मूल्य);
        जर (r < 0)
                परत log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), ओळ, मूल्य);

        free_and_replace(मूल्य, expanded_मूल्य);

        log_debug("%s:%u: setting %s=%s", filename, ओळ, किल्ली, मूल्य);

        परत load_env_file_ढकला(filename, ओळ, किल्ली, मूल्य, env);
}

पूर्णांक merge_env_file(
                अक्षर ***env,
                खाता *f,
                स्थिरांक अक्षर *fname) {

        प्रतिज्ञा(env);
        प्रतिज्ञा(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        परत parse_env_file_internal(f, fname, merge_env_file_ढकला, env);
}

स्थिर शून्य env_file_fputs_escaped(खाता *f, स्थिरांक अक्षर *p) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(p);

        flockfile(f);

        जर (string_has_cc(p, रिक्त) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                साठी (; *p; p++) {
                        जर (म_अक्षर(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } अन्यथा
                fputs_unlocked(p, f);

        funlockfile(f);
}

शून्य env_file_fputs_assignment(खाता *f, स्थिरांक अक्षर *k, स्थिरांक अक्षर *v) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(k);

        जर (!v)
                परत;

        ख_माला_दो(k, f);
        env_file_fputs_escaped(f, v);
        ख_अक्षर_दो('\n', f);
}

स्थिर शून्य write_env_var(खाता *f, स्थिरांक अक्षर *v) {
        स्थिरांक अक्षर *p;

        प्रतिज्ञा(f);
        प्रतिज्ञा(v);

        p = म_अक्षर(v, '=');
        जर (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                परत;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

पूर्णांक write_env_file_label(पूर्णांक dir_fd, स्थिरांक अक्षर *fname, अक्षर **headers, अक्षर **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(fname);

        जर (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                जर (r < 0)
                        परत r;
        }

        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_free_ अक्षर *p = रिक्त;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        जर (r < 0) {
                जर (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (शून्य) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                परत r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        जर (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ रिक्त, /* created= */ true, label_context);
                जर (r < 0)
                        परत r;
        }

        r = fchmod_umask(fileno(f), 0644);
        जर (r < 0)
                परत r;

        STRV_FOREACH(i, headers) {
                प्रतिज्ञा(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        जर (r < 0)
                परत r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        परत 0;
}

पूर्णांक write_vconsole_conf(पूर्णांक dir_fd, स्थिरांक अक्षर *fname, अक्षर **l) {
        अक्षर **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        परत write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ रिक्त);
}
